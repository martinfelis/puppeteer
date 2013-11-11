return {
  points = {
  {
      name = "right_heel",
      point = {
 -0.074956921309592, 0, -0.031668,      },
      body = "foot.r",
    },
  {
      name = "right_hallux",
      point = {
 0.17489948305572, 0, -0.031668,      },
      body = "foot.r",
    },
  {
      name = "left_heel",
      point = {
 -0.074956921309592, 0, -0.031668,      },
      body = "foot.l",
    },
  {
      name = "left_hallux",
      point = {
 0.17489948305572, 0, -0.031668,      },
      body = "foot.l",
    },
  },
  configuration = {
    axis_right = {
 0, -1, 0,    },
    axis_front = {
 1, 0, 0,    },
    axis_up = {
 0, 0, 1,    },
  },
  gravity = {
 0, 0, -9.81,  },
  frames = {
  {
      visuals = {
      {
          mesh_center = {
 0, 0, 0.072808156232051,          },
          name = "lower_trunk",
          color = {
 0.2, 0.2, 0.9,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.388368, 0.1456163124641,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, 0,        },
      },
      name = "pelvis",
      parent = "ROOT",
      joint = {
      {
 0, 0, 0, 1, 0, 0,        },
      {
 0, 0, 0, 0, 1, 0,        },
      {
 0, 0, 0, 0, 0, 1,        },
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.089554032165422, 0, 0,          },
        {
 0, 0.08023458816772, 0,          },
        {
 0, 0, 0.085476775416427,          },
        },
        mass = 8.2658,
        com = {
 0, 0, 0.089044375071798,        },
        length = 0.1456163124641,
        name = "lower_trunk",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.210975,          },
          name = "thigh",
          color = {
 1, 0.1, 0.1,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.42195,          },
        },
      },
      joint_frame = {
        r = {
 0, -0.08091, 0,        },
      },
      name = "thigh.r",
      parent = "pelvis",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.13882155, 0, 0,          },
        {
 0, 0.13882155, 0,          },
        {
 0, 0, 0.06287055,          },
        },
        mass = 10.4784,
        com = {
 0, 0, -0.172788525,        },
        length = 0.42195,
        name = "thigh",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.220023,          },
          name = "shank",
          color = {
 1, 0.2, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.440046,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.42195,        },
      },
      name = "shank.r",
      parent = "thigh.r",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.11221173, 0, 0,          },
        {
 0, 0.109571454, 0,          },
        {
 0, 0, 0.045324738,          },
        },
        mass = 3.2042,
        com = {
 0, 0, -0.1961725068,        },
        length = 0.440046,
        name = "shank",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0.049971280873061, 0, -0.015834,          },
          name = "foot",
          color = {
 1, 0.2, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.24985640436531, 0.1914, 0.031668,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.440046,        },
      },
      name = "foot.r",
      parent = "shank.r",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.064213095921884, 0, 0,          },
        {
 0, 0.0612148190695, 0,          },
        {
 0, 0, 0.030982194141298,          },
        },
        mass = 1.0138,
        com = {
 0.099942561746123, 0, -0.11031160252728,        },
        length = 0.24985640436531,
        name = "foot",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.210975,          },
          name = "thigh",
          color = {
 0.1, 1, 0.1,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.42195,          },
        },
      },
      joint_frame = {
        r = {
 0, 0.08091, 0,        },
      },
      name = "thigh.l",
      parent = "pelvis",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.13882155, 0, 0,          },
        {
 0, 0.13882155, 0,          },
        {
 0, 0, 0.06287055,          },
        },
        mass = 10.4784,
        com = {
 0, 0, -0.172788525,        },
        length = 0.42195,
        name = "thigh",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.220023,          },
          name = "shank",
          color = {
 0.2, 1, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.440046,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.42195,        },
      },
      name = "shank.l",
      parent = "thigh.l",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.11221173, 0, 0,          },
        {
 0, 0.109571454, 0,          },
        {
 0, 0, 0.045324738,          },
        },
        mass = 3.2042,
        com = {
 0, 0, -0.1961725068,        },
        length = 0.440046,
        name = "shank",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0.049971280873061, 0, -0.015834,          },
          name = "foot",
          color = {
 0.2, 1, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.24985640436531, 0.1914, 0.031668,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.440046,        },
      },
      name = "foot.l",
      parent = "shank.l",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.064213095921884, 0, 0,          },
        {
 0, 0.0612148190695, 0,          },
        {
 0, 0, 0.030982194141298,          },
        },
        mass = 1.0138,
        com = {
 0.099942561746123, 0, -0.11031160252728,        },
        length = 0.24985640436531,
        name = "foot",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, 0.10768811028145,          },
          name = "middle_trunk",
          color = {
 0.2, 1, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.420732, 0.21537622056289,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, 0.1456163124641,        },
      },
      name = "middle_trunk",
      parent = "pelvis",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.10381133831132, 0, 0,          },
        {
 0, 0.082489092475589, 0,          },
        {
 0, 0, 0.10079607122343,          },
        },
        mass = 12.0842,
        com = {
 0, 0, 0.096962374497415,        },
        length = 0.21537622056289,
        name = "middle_trunk",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, 0.12098047099368,          },
          name = "upper_trunk",
          color = {
 0.2, 1, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.453096, 0.24196094198736,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, 0.21537622056289,        },
      },
      name = "upper_trunk",
      parent = "middle_trunk",
      joint = {
      },
      body = {
        inertia = {
        {
 0.12219027570362, 0, 0,          },
        {
 0, 0.077427501435956, 0,          },
        {
 0, 0, 0.11251183802412,          },
        },
        mass = 11.8104,
        com = {
 0, 0, 0.1225774132108,        },
        length = 0.24196094198736,
        name = "upper_trunk",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.140766,          },
          name = "upper_arm",
          color = {
 1, 0.1, 0.1,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.281532,          },
        },
      },
      joint_frame = {
        r = {
 0, -0.208887, 0.24196094198736,        },
      },
      name = "upper_arm.r",
      parent = "upper_trunk",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.08023662, 0, 0,          },
        {
 0, 0.075732108, 0,          },
        {
 0, 0, 0.044482056,          },
        },
        mass = 2.0054,
        com = {
 0, 0, -0.1625002704,        },
        length = 0.281532,
        name = "upper_arm",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.134328,          },
          name = "lower_arm",
          color = {
 1, 0.2, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.268656,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.281532,        },
      },
      name = "lower_arm.r",
      parent = "upper_arm.r",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.074149056, 0, 0,          },
        {
 0, 0.07119384, 0,          },
        {
 0, 0, 0.032507376,          },
        },
        mass = 1.1988,
        com = {
 0, 0, -0.1228832544,        },
        length = 0.268656,
        name = "lower_arm",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.043065,          },
          name = "hand",
          color = {
 1, 0.3, 0.3,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.08613,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.268656,        },
      },
      name = "hand.r",
      parent = "lower_arm.r",
      joint = {
      },
      body = {
        inertia = {
        {
 0.05408964, 0, 0,          },
        {
 0, 0.04418469, 0,          },
        {
 0, 0, 0.03453813,          },
        },
        mass = 0.4514,
        com = {
 0, 0, -0.0680427,        },
        length = 0.08613,
        name = "hand",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.140766,          },
          name = "upper_arm",
          color = {
 0.1, 1, 0.1,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.281532,          },
        },
      },
      joint_frame = {
        r = {
 0, 0.208887, 0.24196094198736,        },
      },
      name = "upper_arm.l",
      parent = "upper_trunk",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.08023662, 0, 0,          },
        {
 0, 0.075732108, 0,          },
        {
 0, 0, 0.044482056,          },
        },
        mass = 2.0054,
        com = {
 0, 0, -0.1625002704,        },
        length = 0.281532,
        name = "upper_arm",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.134328,          },
          name = "lower_arm",
          color = {
 0.2, 1, 0.2,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.268656,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.281532,        },
      },
      name = "lower_arm.l",
      parent = "upper_arm.l",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.074149056, 0, 0,          },
        {
 0, 0.07119384, 0,          },
        {
 0, 0, 0.032507376,          },
        },
        mass = 1.1988,
        com = {
 0, 0, -0.1228832544,        },
        length = 0.268656,
        name = "lower_arm",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, -0.043065,          },
          name = "hand",
          color = {
 0.3, 1, 0.3,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.08613,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, -0.268656,        },
      },
      name = "hand.l",
      parent = "lower_arm.l",
      joint = {
      },
      body = {
        inertia = {
        {
 0.05408964, 0, 0,          },
        {
 0, 0.04418469, 0,          },
        {
 0, 0, 0.03453813,          },
        },
        mass = 0.4514,
        com = {
 0, 0, -0.0680427,        },
        length = 0.08613,
        name = "hand",
      },
    },
  {
      visuals = {
      {
          mesh_center = {
 0, 0, 0.121365,          },
          name = "head",
          color = {
 0.1, 0.1, 1,          },
          src = "meshes/unit_cube.obj",
          dimensions = {
 0.1392, 0.1392, 0.24273,          },
        },
      },
      joint_frame = {
        r = {
 0, 0, 0.24196094198736,        },
      },
      name = "head",
      parent = "upper_trunk",
      joint = {
      {
 0, 1, 0, 0, 0, 0,        },
      {
 1, 0, 0, 0, 0, 0,        },
      {
 0, 0, 1, 0, 0, 0,        },
      },
      body = {
        inertia = {
        {
 0.07354719, 0, 0,          },
        {
 0, 0.07645995, 0,          },
        {
 0, 0, 0.06335253,          },
        },
        mass = 5.1356,
        com = {
 0, 0, 0.121413546,        },
        length = 0.24273,
        name = "head",
      },
    },
  },
  constraint_sets = {
    RightHalxLeftHeel = {
    {
        point = "right_hallux",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "right_hallux",
        normal = {
 0, 0, 1,        },
      },
    {
        point = "left_heel",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "left_heel",
        normal = {
 0, 0, 1,        },
      },
    },
    RightFlat = {
    {
        point = "right_heel",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "right_heel",
        normal = {
 0, 0, 1,        },
      },
    {
        point = "right_hallux",
        normal = {
 0, 0, 1,        },
      },
    },
    RightHalxLeftFlat = {
    {
        point = "right_hallux",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "right_hallux",
        normal = {
 0, 0, 1,        },
      },
    {
        point = "left_heel",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "left_heel",
        normal = {
 0, 0, 1,        },
      },
    {
        point = "left_hallux",
        normal = {
 0, 0, 1,        },
      },
    },
    RightHalx = {
    {
        point = "right_hallux",
        normal = {
 1, 0, 0,        },
      },
    {
        point = "right_hallux",
        normal = {
 0, 0, 1,        },
      },
    },
  },
}