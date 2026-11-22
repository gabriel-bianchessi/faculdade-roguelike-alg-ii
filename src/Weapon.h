#pragma once

enum class WeaponCategory { Melee, Ranged };
enum class WeaponType { Sword, Pistol, Spear, Shotgun };

struct WeaponConfig
{
    WeaponType type;
    WeaponCategory category;
    int damage;
    float interval;
    float reach;
    float width;
    float projectileSpeed;
    float projectileLifetime;
    int projectileCount = 1;
    float spread = 0;
};

inline constexpr WeaponConfig sword{WeaponType::Sword, WeaponCategory::Melee, 20, 0.4f, 76, 64, 0, 0};
inline constexpr WeaponConfig pistol{WeaponType::Pistol, WeaponCategory::Ranged, 8, 0.3f, 0, 0, 650, 2};

inline constexpr WeaponConfig spear{WeaponType::Spear, WeaponCategory::Melee, 28, 0.65f, 130, 28, 0, 0};
inline constexpr WeaponConfig shotgun{WeaponType::Shotgun, WeaponCategory::Ranged, 6, 0.9f, 0, 0, 650, 2, 5, 0.5f};

inline const WeaponConfig& weaponConfig(WeaponType type)
{
    switch (type)
    {
        case WeaponType::Sword: return sword;
        case WeaponType::Pistol: return pistol;
        case WeaponType::Spear: return spear;
        case WeaponType::Shotgun: return shotgun;
    }
    return sword;
}

inline const char* weaponName(WeaponType type)
{
    switch (type)
    {
        case WeaponType::Sword: return "Espada";
        case WeaponType::Pistol: return "Pistola";
        case WeaponType::Spear: return "Lanca";
        case WeaponType::Shotgun: return "Escopeta";
    }
    return "";
}

inline int magazineCapacity(WeaponType type)
{
    return type == WeaponType::Pistol ? 15 : type == WeaponType::Shotgun ? 7 : 0;
}

inline float reloadDuration(WeaponType type)
{
    return type == WeaponType::Pistol ? 1.2f : type == WeaponType::Shotgun ? 1.8f : 0;
}
