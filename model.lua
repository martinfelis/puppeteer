-- load HeiMan and additional helper modules
HeiMan = require 'HeiMan.HeiMan'
serialize = require 'HeiMan.serialize'

-- create actual HeiMan instance by specifying the mass and height
heiman = HeiMan (74, 1.74)

-- modify parameters

-- heiman.parameters["ClaviculaLeftLength"]     = 0.208800
-- heiman.parameters["ClaviculaRightLength"]    = 0.208800
-- heiman.parameters["FootLeftHeight"]          = 0.081432
-- heiman.parameters["FootLeftLength"]          = 0.254388
-- heiman.parameters["FootLeftMass"]            = 1.013800
-- heiman.parameters["FootLeftPosteriorPoint"]  = 0.069600
-- heiman.parameters["FootRightHeight"]         = 0.081432
-- heiman.parameters["FootRightLength"]         = 0.254388
-- heiman.parameters["FootRightMass"]           = 1.013800
-- heiman.parameters["FootRightPosteriorPoint"] = 0.069600
-- heiman.parameters["HandLeftLength"]          = 0.187792
-- heiman.parameters["HandLeftMass"]            = 0.451400
-- heiman.parameters["HandRightLength"]         = 0.187792
-- heiman.parameters["HandRightMass"]           = 0.451400
-- heiman.parameters["HeadLength"]              = 0.203183
-- heiman.parameters["HeadMass"]                = 5.135600
-- heiman.parameters["HipLeftWidth"]            = 0.080214
-- heiman.parameters["HipRightWidth"]           = 0.080214
-- heiman.parameters["LowerArmLeftLength"]      = 0.268746
-- heiman.parameters["LowerArmLeftMass"]        = 1.198800
-- heiman.parameters["LowerArmRightLength"]     = 0.268746
-- heiman.parameters["LowerArmRightMass"]       = 1.198800
-- heiman.parameters["LowerTrunkHeight"]        = 0.145616
-- heiman.parameters["LowerTrunkMass"]          = 8.265800
-- heiman.parameters["MiddleTrunkHeight"]       = 0.215376
-- heiman.parameters["MiddleTrunkMass"]         = 12.084200
-- heiman.parameters["NeckLength"]              = 0.110936
-- heiman.parameters["NeckMass"]                = 0.495800
-- heiman.parameters["ShankLeftLength"]         = 0.440047
-- heiman.parameters["ShankLeftMass"]           = 3.204200
-- heiman.parameters["ShankRightLength"]        = 0.440047
-- heiman.parameters["ShankRightMass"]          = 3.204200
-- heiman.parameters["ShoulderLeftHeight"]      = 0.170602
-- heiman.parameters["ShoulderRightHeight"]     = 0.170602
-- heiman.parameters["SuprasternaleHeight"]     = 0.170602
-- heiman.parameters["ThighLeftLength"]         = 0.421957
-- heiman.parameters["ThighLeftMass"]           = 10.478400
-- heiman.parameters["ThighRightLength"]        = 0.421957
-- heiman.parameters["ThighRightMass"]          = 10.478400
-- heiman.parameters["UpperArmLeftLength"]      = 0.281538
-- heiman.parameters["UpperArmLeftMass"]        = 2.005400
-- heiman.parameters["UpperArmRightLength"]     = 0.281538
-- heiman.parameters["UpperArmRightMass"]       = 2.005400
-- heiman.parameters["UpperTrunkAltHeight"]     = 0.241961
-- heiman.parameters["UpperTrunkAltMass"]       = 11.810400
-- heiman.parameters["UpperTrunkHeight"]        = 0.170602
-- heiman.parameters["UpperTrunkMass"]          = 11.810400

-- create the actual model (must be called whenever parameters were modified)
model = heiman:create_model()

-- optional: convert it to text so that you can inspect it manually
-- out = serialize( model )
-- print (out)

-- very important: return the created model
return model
