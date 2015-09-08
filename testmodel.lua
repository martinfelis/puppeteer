return {
  frames = {
    {
      body = {
        com = { 0, 0, 0.042646321347356,},
        inertia = {
          { 0.038200528883491, 0, 0,},
          { 0, 0.030663543577386, 0,},
          { 0, 0, 0.034801290334736,},
        },
        mass = 8.3775,
      },
      joint = {
        { 0, 0, 0, 1, 0, 0,},
        { 0, 0, 0, 0, 1, 0,},
        { 0, 0, 0, 0, 0, 1,},
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, 1.0,},
      },
      markers = {
        LASI = { 0.052991829812527, 0.11058157682419, 0.08067062497139,},
        LPSI = { -0.12932787835598, 0.045250821858644, 0.11889722943306,},
        RASI = { 0.058340698480606, -0.11695376038551, 0.089762836694717,},
        RPSI = { -0.1268265992403, -0.058085463941097, 0.12065640091896,},
      },
      name = "Pelvis",
      parent = "ROOT",
      visuals = {
        {
          color = { 0.2, 0.2, 0.9,},
          dimensions = { 0.18, 0.28647, 0.10980000346899,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, 0.054900001734495,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.18185894693434,},
        inertia = {
          { 0.22671374155723, 0, 0,},
          { 0, 0.22671374155723, 0,},
          { 0, 0, 0.04650060306457,},
        },
        mass = 10.62,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, -0.083235, 0,},
      },
      name = "ThighRight",
      parent = "Pelvis",
      visuals = {
        {
          color = { 1, 0.1, 0.1,},
          dimensions = { 0.15, 0.12, 0.44409999251366,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.22204999625683,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.20627166416645,},
        inertia = {
          { 0.045209378540837, 0, 0,},
          { 0, 0.043106907787935, 0,},
          { 0, 0, 0.0073760291724681,},
        },
        mass = 3.2475,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.44409999251366,},
      },
      markers = {
        RKNE = { 0.031680159270763, -0.064131885766983, 0.0048278677277267,},
        RTIB = { 0.011271089315414, -0.097036071121693, -0.11671927571297,},
      },
      name = "ShankRight",
      parent = "ThighRight",
      visuals = {
        {
          color = { 0.9, 0.3, 0.3,},
          dimensions = { 0.13, 0.1, 0.46270000934601,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.23135000467301,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.038431692,},
        inertia = {
          { 0.0050184397083146, 0, 0,},
          { 0, 0.0045607328421564, 0,},
          { 0, 0, 0.0011682770209246,},
        },
        mass = 1.0275,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.46270000934601,},
      },
      markers = {
        RANK = { 0.0054707257077098, -0.030474988743663, 0.0096247950568795,},
        RHEE = { -0.053590387105942, 0.006934370379895, -0.01508407574147,},
        RTOE = { 0.17356503009796, 0.011473462916911, -0.024925166741014,},
      },
      name = "FootRight",
      parent = "ShankRight",
      visuals = {
        {
          color = { 1, 0.1, 0.1,},
          dimensions = { 0.271932, 0.1, 0.087048,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
              segments = 48,
            },
          },
          mesh_center = { 0.061566, 0, -0.043524,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.18185894693434,},
        inertia = {
          { 0.22671374155723, 0, 0,},
          { 0, 0.22671374155723, 0,},
          { 0, 0, 0.04650060306457,},
        },
        mass = 10.62,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0.083235, 0,},
      },
      name = "ThighLeft",
      parent = "Pelvis",
      visuals = {
        {
          color = { 0.1, 1, 0.1,},
          dimensions = { 0.15, 0.12, 0.44409999251366,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.22204999625683,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.20627166416645,},
        inertia = {
          { 0.045209378540837, 0, 0,},
          { 0, 0.043106907787935, 0,},
          { 0, 0, 0.0073760291724681,},
        },
        mass = 3.2475,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.44409999251366,},
      },
      markers = {
        LKNE = { 0.032328642904758, 0.047998454421759, -0.0018083926988766,},
        LTIB = { 0.0066562313586473, 0.065629869699478, -0.29721513390541,},
      },
      name = "ShankLeft",
      parent = "ThighLeft",
      visuals = {
        {
          color = { 0.3, 0.9, 0.3,},
          dimensions = { 0.13, 0.1, 0.46270000934601,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.23135000467301,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.038431692,},
        inertia = {
          { 0.0050184397083146, 0, 0,},
          { 0, 0.0045607328421564, 0,},
          { 0, 0, 0.0011682770209246,},
        },
        mass = 1.0275,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.46270000934601,},
      },
      markers = {
        LANK = { 0.0081671224907041, 0.034606732428074, 0.0077109434641898,},
        LHEE = { -0.051829442381859, -0.016891548410058, -0.013164701871574,},
        LTOE = { 0.16798159480095, -0.0085604833438993, -0.017251666635275,},
      },
      name = "FootLeft",
      parent = "ShankLeft",
      visuals = {
        {
          color = { 0.1, 1, 0.1,},
          dimensions = { 0.271932, 0.1, 0.087048,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
              segments = 48,
            },
          },
          mesh_center = { 0.061566, 0, -0.043524,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0.1218165427915,},
        inertia = {
          { 0.13968333431522, 0, 0,},
          { 0, 0.088195832661997, 0,},
          { 0, 0, 0.13168679350845,},
        },
        mass = 12.2475,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, 0.10980000346899,},
      },
      name = "MiddleTrunk",
      parent = "Pelvis",
      visuals = {
        {
          color = { 0.3, 0.3, 1,},
          dimensions = { 0.216, 0.29189999602795, 0.22156519241815,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, 0.11078259620908,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0.16375339219495,},
        inertia = {
          { 0.33572265599833, 0, 0,},
          { 0, 0.13497903265299, 0,},
          { 0, 0, 0.28439727764741,},
        },
        mass = 11.97,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, 0.22156519241815,},
      },
      markers = {
        C7 = { -0.039351474493742, -0.0084890006110072, 0.28900960087776,},
        T10 = { -0.13401326537132, -0.010717937722802, 0.0360297113657,},
      },
      name = "UpperTrunk",
      parent = "MiddleTrunk",
      visuals = {
        {
          color = { 0.1, 0.1, 1,},
          dimensions = { 0.234, 0.2973299920559, 0.2339000031352,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, 0.1169500015676,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0,},
        inertia = {
          { 0, 0, 0,},
          { 0, 0, 0,},
          { 0, 0, 0,},
        },
        mass = 0,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, 0.228900000453,},
      },
      name = "ClaviculaRight",
      parent = "UpperTrunk",
      visuals = {
        {
          color = { 0.3, 0.3, 1,},
          dimensions = { 0.06, 0.06, 0.174899995327,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, -0.0874499976635, 0,},
          rotate = {
            angle = 90,
            axis = { 1, 0, 0,},
          },
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.21910512,},
        inertia = {
          { 0.02378880803637, 0, 0,},
          { 0, 0.021192760090117, 0,},
          { 0, 0, 0.0073113426139728,},
        },
        mass = 2.0325,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, -0.174899995327, 0,},
      },
      markers = {
        RUPA = { -0.0080910492688417, -0.03629956394434, -0.27596998214722,},
      },
      name = "UpperArmRight",
      parent = "ClaviculaRight",
      visuals = {
        {
          color = { 1, 0.1, 0.1,},
          dimensions = { 0.12, 0.09, 0.3796,},
          geometry = {
            capsule = {
              length = 0.88,
              radius = 0.12,
            },
          },
          mesh_center = { 0, 0, -0.1898,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.124133786,},
        inertia = {
          { 0.0068168246715783, 0, 0,},
          { 0, 0.0062842826160678, 0,},
          { 0, 0, 0.0013101912678085,},
        },
        mass = 1.215,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.3796,},
      },
      markers = {
        RELB = { -0.0073555246926844, -0.04216194152832, 0.0015217624604702,},
        RWRA = { 0.045201644301414, 0.039090558886528, -0.27397903800011,},
        RWRB = { -0.015719685703516, -0.017938679084182, -0.28111892938614,},
      },
      name = "LowerArmRight",
      parent = "UpperArmRight",
      visuals = {
        {
          color = { 1, 0.3, 0.3,},
          dimensions = { 0.1, 0.08, 0.27139,},
          geometry = {
            capsule = {
              length = 0.88,
              radius = 0.12,
            },
          },
          mesh_center = { 0, 0, -0.135695,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.05189568,},
        inertia = {
          { 0.0007781478285312, 0, 0,},
          { 0, 0.00051809912508, 0,},
          { 0, 0, 0.0003176236121088,},
        },
        mass = 0.4575,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, -0.27139,},
      },
      name = "HandRight",
      parent = "LowerArmRight",
      visuals = {
        {
          color = { 1, 0.1, 0.1,},
          dimensions = { 0.1, 0.04, 0.1432,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.0716,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0,},
        inertia = {
          { 0, 0, 0,},
          { 0, 0, 0,},
          { 0, 0, 0,},
        },
        mass = 0,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, 0.228900000453,},
      },
      name = "ClaviculaLeft",
      parent = "UpperTrunk",
      visuals = {
        {
          color = { 0.3, 0.3, 1,},
          dimensions = { 0.06, 0.06, 0.174899995327,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0.0874499976635, 0,},
          rotate = {
            angle = 90,
            axis = { 1, 0, 0,},
          },
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.21910512,},
        inertia = {
          { 0.02378880803637, 0, 0,},
          { 0, 0.021192760090117, 0,},
          { 0, 0, 0.0073113426139728,},
        },
        mass = 2.0325,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0.174899995327, 0,},
      },
      markers = {
        LUPA = { -0.0083068665117025, 0.041783545166254, -0.16908901929855,},
      },
      name = "UpperArmLeft",
      parent = "ClaviculaLeft",
      visuals = {
        {
          color = { 0.1, 1, 0.1,},
          dimensions = { 0.12, 0.09, 0.3796,},
          geometry = {
            capsule = {
              length = 0.88,
              radius = 0.12,
            },
          },
          mesh_center = { 0, 0, -0.1898,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.124133786,},
        inertia = {
          { 0.0068168246715783, 0, 0,},
          { 0, 0.0062842826160678, 0,},
          { 0, 0, 0.0013101912678085,},
        },
        mass = 1.215,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, -0.3796,},
      },
      markers = {
        LELB = { -0.0020065398421139, 0.043605700135231, 0.0037837768904865,},
        LWRA = { 0.02726911008358, -0.025259353220463, -0.26385623216629,},
        LWRB = { -0.035459812730551, 0.032188482582569, -0.26435285806656,},
      },
      name = "LowerArmLeft",
      parent = "UpperArmLeft",
      visuals = {
        {
          color = { 0.3, 0.9, 0.3,},
          dimensions = { 0.1, 0.08, 0.27139,},
          geometry = {
            capsule = {
              length = 0.88,
              radius = 0.12,
            },
          },
          mesh_center = { 0, 0, -0.135695,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, -0.05189568,},
        inertia = {
          { 0.0007781478285312, 0, 0,},
          { 0, 0.00051809912508, 0,},
          { 0, 0, 0.0003176236121088,},
        },
        mass = 0.4575,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, -0.27139,},
      },
      name = "HandLeft",
      parent = "LowerArmLeft",
      visuals = {
        {
          color = { 0.1, 1, 0.1,},
          dimensions = { 0.1, 0.04, 0.1432,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, -0.0716,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0.017902498432397,},
        inertia = {
          { 4.9072875886111e-05, 0, 0,},
          { 0, 4.5239218508482e-05, 0,},
          { 0, 0, 9.4317892229646e-06,},
        },
        mass = 0.5025,
      },
      joint = {},
      joint_frame = {
        r = { 0, 0, 0.2339000031352,},
      },
      name = "Neck",
      parent = "UpperTrunk",
      visuals = {
        {
          color = { 0.2, 0.2, 0.9,},
          dimensions = { 0.07, 0.07, 0.035804996864795,},
          geometry = {
            capsule = {
              length = 0.9,
              radius = 0.09,
            },
          },
          mesh_center = { 0, 0, 0.017902498432397,},
        },
      },
    },
    {
      body = {
        com = { 0, 0, 0.088409292,},
        inertia = {
          { 0.032924391420566, 0, 0,},
          { 0, 0.035520273812414, 0,},
          { 0, 0, 0.024457372778941,},
        },
        mass = 5.205,
      },
      joint = {
        { 0, 1, 0, 0, 0, 0,},
        { 1, 0, 0, 0, 0, 0,},
        { 0, 0, 1, 0, 0, 0,},
      },
      joint_frame = {
        r = { 0, 0, 0.035804996864795,},
      },
      markers = {
        LBHD = { 0.019333571195602, 0.088616423308849, 0.20507903397083,},
        LFHD = { 0.12930905818939, 0.067053556442261, 0.20482148230076,},
        RBHD = { 0.011489745229483, -0.087685711681843, 0.19465588033199,},
        RFHD = { 0.12186269462109, -0.073935218155384, 0.1992692053318,},
      },
      name = "Head",
      parent = "Neck",
      visuals = {
        {
          color = { 0.1, 0.1, 0.9,},
          dimensions = { 0.2, 0.18, 0.2416755,},
          geometry = {
            capsule = {
              length = 0.8,
              radius = 0.2,
            },
          },
          mesh_center = { 0, 0, 0.09886725,},
        },
      },
    },
  },
  gravity = { 0, 0, -9.81,},
  parameters = {
    ClaviculaLeftLength = 0.174899995327,
    ClaviculaRightLength = 0.174899995327,
    FootLeftHeight = 0.087048,
    FootLeftLength = 0.271932,
    FootLeftMass = 1.0275,
    FootLeftPosteriorPoint = 0.0744,
    FootRightHeight = 0.087048,
    FootRightLength = 0.271932,
    FootRightMass = 1.0275,
    FootRightPosteriorPoint = 0.0744,
    HandLeftLength = 0.1432,
    HandLeftMass = 0.4575,
    HandRightLength = 0.1432,
    HandRightMass = 0.4575,
    HeadLength = 0.219705,
    HeadMass = 5.205,
    HipLeftWidth = 0.083235,
    HipRightWidth = 0.083235,
    LowerArmLeftLength = 0.27139,
    LowerArmLeftMass = 1.215,
    LowerArmRightLength = 0.27139,
    LowerArmRightMass = 1.215,
    LowerTrunkHeight = 0.10980000346899,
    LowerTrunkMass = 8.3775,
    MiddleTrunkHeight = 0.22156519241815,
    MiddleTrunkMass = 12.2475,
    NeckLength = 0.035804996864795,
    NeckMass = 0.5025,
    ShankLeftLength = 0.46270000934601,
    ShankLeftMass = 3.2475,
    ShankRightLength = 0.46270000934601,
    ShankRightMass = 3.2475,
    ShoulderLeftHeight = 0.228900000453,
    ShoulderRightHeight = 0.228900000453,
    SuprasternaleHeight = 0.2339000031352,
    ThighLeftLength = 0.44409999251366,
    ThighLeftMass = 10.62,
    ThighRightLength = 0.44409999251366,
    ThighRightMass = 10.62,
    UpperArmLeftLength = 0.3796,
    UpperArmLeftMass = 2.0325,
    UpperArmRightLength = 0.3796,
    UpperArmRightMass = 2.0325,
    UpperTrunkAltHeight = 0.25864790350373,
    UpperTrunkAltMass = 11.97,
    UpperTrunkHeight = 0.2339000031352,
    UpperTrunkMass = 11.97,
  },
  points = {
    {
      body = "FootRight",
      name = "right_heel",
      point = { -0.035, 0, -0.087048,},
    },
    {
      body = "FootRight",
      name = "right_hallux",
      point = { 0.18, 0.03, -0.087048,},
    },
    {
      body = "FootRight",
      name = "right_meta5",
      point = { 0.18, -0.06, -0.087048,},
    },
    {
      body = "FootLeft",
      name = "left_heel",
      point = { -0.035, 0, -0.087048,},
    },
    {
      body = "FootLeft",
      name = "left_hallux",
      point = { 0.18, -0.03, -0.087048,},
    },
    {
      body = "FootLeft",
      name = "left_meta5",
      point = { 0.18, 0.06, -0.087048,},
    },
  },
}
