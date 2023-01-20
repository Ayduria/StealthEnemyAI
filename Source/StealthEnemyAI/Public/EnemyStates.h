#pragma once

/*
 * Base behavior of enemy
 * Stationary by default, can be configured in editor
 */
UENUM(BlueprintType)
enum class EEnemyBehavior : uint8
{
	Stationary			UMETA(DisplayName = "Stationary"), // Enemy stays in a fixed position
	Patrol				UMETA(DisplayName = "Patrol"), // Enemy patrols from waypoint to waypoint
};

/*
 * Behavior of enemy when reaching end of patrol route
 * Reverse by default, can be configured in editor
 */
UENUM(BlueprintType)
enum class EEnemyPatrolMode : uint8
{
	Reverse				UMETA(DisplayName = "Back and forth"), // Enemy patrols route again in a backwards order
	Loop				UMETA(DisplayName = "Loop"), // Enemy patrols route again in the right order
};

/*
 * Current state of enemy
 * Guarding / Patrolling by default, set by controller
 */
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Guarding			UMETA(DisplayName = "Guarding"), // Base state of an enemy with Stationary behavior
	Patrolling			UMETA(DisplayName = "Patrolling"), // Base state of an enemy with Patrol behavior
	Suspicious			UMETA(DisplayName = "Suspicious"), // State triggered by enemy hearing a noise
	Investigating		UMETA(DisplayName = "Investigating"), // State triggered after short delay upon Suspicious state trigger
	Chasing				UMETA(DisplayName = "Chasing"), // State triggered when enemy has target in line of sight
	Resuming			UMETA(DisplayName = "Resuming activities"), // State triggered when enemy does not find target during investigation
	Stunned				UMETA(DisplayName = "Stunned"), // State triggered when enemy is in an incapacitated state
};