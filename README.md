# Miner Minors

![Unreal Engine 5.6](https://img.shields.io/badge/Unreal%20Engine-5.6-black?logo=unrealengine)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Blueprint-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

A first-person multiplayer loot-extraction game built in Unreal Engine 5.6. Players explore underground environments, mine gem nodes, harvest resources, and manage an inventory of consumables, upgrades, and valuables — all while co-operatively dragging heavy objects through physics-based grabbing.

---

## Key Features

- **First-person exploration** with a toggleable spotlight/flashlight
- **Physics-based grab system** — pick up and haul objects using spring-damper force controllers; multiple players can share a grab target (`GrabForceComponent`, `FSharedTarget`)
- **Stat system** — Health, Stamina, Power, and Strength each tick per-frame with configurable recovery rates, consumable scaling, and permanent upgrade modifiers (`StatlineComponent`, `FCoreStat`)
- **Inventory** — stackable item slots with a dedicated gem slot, broadcasting changes to UI via delegate (`InventoryComponent`)
- **Unified interaction system** — one interface (`IInteractionInterface`) drives pickups, harvestable resources, and doors
- **Gem mining** — gem nodes absorb damage until their health pool empties, then spawn pickup actors at designer-defined transforms (`GemBase`)
- **Regrowing harvestables** — resources respawn after a configurable number of in-game days (`RegrowingHarvestableActorBase`)
- **Item hierarchy** — Base → Consumable (stat adjustments on use) / Upgrade (permanent stat changes) / Value (loot/currency)
- **StateTree AI** — AI behaviour powered by Unreal's StateTree and GameplayStateTree plugins

---

## Tech Stack

| Layer | Technology |
|---|---|
| Engine | Unreal Engine 5.6 |
| Gameplay code | C++ (hybrid with Blueprint) |
| Input | Enhanced Input (`EnhancedInput` module) |
| UI | UMG (`UMG` module) |
| AI | StateTree + GameplayStateTree plugins |
| IDE | Visual Studio 2022 |

---

## Project Structure

```
Source/MinorMiners/
├── Public/
│   ├── BaseClass/          # Core actor and character base classes
│   │   └── Player/         # Player-specific character class
│   ├── Components/         # Reusable actor components (stats, inventory, grabbing)
│   ├── Items/              # Item class hierarchy
│   ├── DataType/           # Structs and enums used across systems
│   └── Interface/          # IInteractionInterface
└── Private/                # Paired .cpp implementations
```

---

## Architecture Overview

### Character
`AMMCharacter` (extends `ACharacter`) owns a `UStatlineComponent` and a `UInventoryComponent`, providing the stat/inventory foundation for both players and NPCs. `AMMPlayerCharacter` extends it with a first-person camera, spotlight, Enhanced Input bindings, interaction detection, and the grab system.

### Stats
`UStatlineComponent` manages four `FCoreStat` values — each struct tracks a current value, maximum, per-second tick rate, and an amount modifier. Stats recover or drain every frame; consumables scale their adjustment by the stat's current percentile modifier, and upgrade items can raise the maximum or adjust the modifier permanently.

### Inventory
`UInventoryComponent` holds a fixed array of `FInventorySlot` entries. It stacks items automatically, reserves slot index 5 for gems, and fires an `OnInventoryChanged` delegate so UI widgets stay in sync without polling.

### Interaction
`IInteractionInterface` declares three methods — `GetInteractionText`, `Interact`, and `IsInteractable` — implemented by `AMMPickupActorBase`, `AMMHarvestableActorBase`, and `ABasicDoor`. `AMMPlayerCharacter` uses a sphere overlap trigger to maintain a list of nearby interactables and traces for the active one each frame.

### Items
`UItemBase` defines name, icon, weight, stack size, an `FExtractItem` drop table, and a virtual `OnUse`. Subclasses override `OnUse`: `UConsumableItemBase` applies a `TMap<ECoreStat, float>` of stat adjustments; `UUpgradeItemBase` raises stat maxima or modifiers; `UValueItemBase` adds no behaviour (loot/currency).

### Mining & Harvesting
`AGemBase` actors hold a health pool. Any damage source depletes it; at zero, `Harvest()` hides the gem mesh, shows an empty cavity, spawns `AMMPickupActorBase` instances at designer-placed transforms, and calls the `OnHarvestedBP` Blueprint event for effects. Static harvestables (`AMMHarvestableActorBase`) toggle mesh visibility on interact; `ARegrowingHarvestableActorBase` resets after N in-game days.

### Physics Grabbing
`UGrabForceComponent` uses proportional-derivative (PD) controllers to apply forces and torques toward a held target position/rotation, scaled by the character's Strength stat. `FSharedTarget` lets multiple players grab the same object simultaneously — one player leads (updating the target each frame) while others follow.

---

## Known Limitations / In-Progress

- `ARegrowingHarvestableActorBase::OnDayChange()` is written but not yet subscribed to a day-cycle delegate — regrowth timing requires wiring up to a game-mode day system.
- `ABasicDoor::Interact_Implementation()` contains a recursive call that should be replaced with a Blueprint override for open/close logic.
