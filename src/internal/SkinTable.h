#pragma once

// -----------------------------------------------------------------------
// Fixed skin table for the internal module.
//
// There is no UI in this tool — one curated skin per weapon is applied
// automatically. PaintKit ids are taken from the databases already shipped
// in src/features/skinchanger/SkinChanger.cpp so the look matches the
// main product.
//
// Knives are handled separately: ANY knife definition index gets the knife
// entry (m_nKnifePaintKit), because the player only ever holds one knife
// and its defindex depends on the account's inventory.
// -----------------------------------------------------------------------

struct InternalSkin_t
{
    std::uint16_t m_nDefIndex;
    int           m_nPaintKit;
    const char*   m_szName;
};

namespace SkinTable
{
    inline constexpr InternalSkin_t g_Skins[] =
    {
        // --- pistols ---
        { WEAPON_DESERT_EAGLE,  37,  "Deagle | Blaze"            },
        { WEAPON_DUAL_BERETTAS, 565, "Dualies | Cobalt Quartz"   },
        { WEAPON_FIVE_SEVEN,    427, "Five-SeveN | Monkey Bus."  },
        { WEAPON_GLOCK_18,      48,  "Glock-18 | Fade"           },
        { WEAPON_P2000,         389, "P2000 | Fire Elemental"    },
        { WEAPON_P250,          551, "P250 | Asiimov"            },
        { WEAPON_TEC_9,         614, "Tec-9 | Fuel Injector"     },
        { WEAPON_M4A1_S,        910, "M4A1-S | Printstream"      },
        { WEAPON_USP_S,         911, "USP-S | Printstream"       },
        { WEAPON_CZ75_AUTO,     270, "CZ75-Auto | Victoria"      },
        { WEAPON_R8_REVOLVER,   548, "R8 Revolver | Fade"        },

        // --- rifles ---
        { WEAPON_AK_47,         801, "AK-47 | Asiimov"           },
        { WEAPON_M4A4,          309, "M4A4 | Howl"               },
        { WEAPON_AUG,           455, "AUG | Akihabara Accept"    },
        { WEAPON_FAMAS,         523, "FAMAS | Mecha Industries"  },
        { WEAPON_GALIL_AR,      379, "Galil AR | Chatterbox"     },
        { WEAPON_SG_553,        487, "SG 553 | Cyrex"            },

        // --- snipers ---
        { WEAPON_AWP,           344, "AWP | Dragon Lore"         },
        { WEAPON_SSG_08,        624, "SSG 08 | Dragonfire"       },
        { WEAPON_SCAR_20,       597, "SCAR-20 | Bloodsport"      },
        { WEAPON_G3SG1,         511, "G3SG1 | The Executioner"   },

        // --- smgs ---
        { WEAPON_P90,           359, "P90 | Death by Kitty"      },
        { WEAPON_MP9,           510, "MP9 | Airlock"             },
        { WEAPON_MP7,           479, "MP7 | Nemesis"             },
        { WEAPON_MAC_10,        433, "MAC-10 | Neon Rider"       },
        { WEAPON_UMP_45,        556, "UMP-45 | Primal Saber"     },
    };

    inline constexpr int g_nSkinCount = static_cast<int>(sizeof(g_Skins) / sizeof(g_Skins[0]));

    // Applied to whatever knife the account actually owns.
    inline constexpr int         g_nKnifePaintKit = 38;   // Fade
    inline constexpr const char* g_szKnifeName    = "Knife | Fade";

    // Shared cosmetic values for every entry above.
    inline constexpr float g_flWear     = 0.0001f;  // Factory New
    inline constexpr int   g_nSeed      = 0;
    inline constexpr int   g_nStatTrak  = -1;       // -1 = disabled

    // Returns the paint kit for a definition index, or 0 when we have none.
    inline int Lookup(std::uint16_t nDefIndex, const char** pszNameOut = nullptr)
    {
        for (int i = 0; i < g_nSkinCount; ++i)
        {
            if (g_Skins[i].m_nDefIndex == nDefIndex)
            {
                if (pszNameOut) *pszNameOut = g_Skins[i].m_szName;
                return g_Skins[i].m_nPaintKit;
            }
        }

        if (pszNameOut) *pszNameOut = nullptr;
        return 0;
    }
}
