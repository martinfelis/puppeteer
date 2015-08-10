Puppeteer - A Motion Capture Mapping Tool
Copyright (c) 2013-2015 Martin Felis <martin.felis@iwr.uni-heidelberg.de>

Tip of the day: try the new scripting feature by running

    ./puppeteer subject3m.lua -s scripts/motionsickness.lua

# Dependencies

Puppeteer uses the following libraries and includes some of them in the
`vendor/` folder. The following libraries come with the code:

  * c3dfile (c3d file parsing, MIT license)
  * LuaTables++ (LuaModel reading/writing, MIT License)
	* TCLAP (command line parsing, MIT license)
	* QtPropertyBrowser (Property widget, BSD license)

In addition Puppeteer uses RBDL, the Rigid Body Dynamics Library that you
can obtain from https://rbdl.bitbucket.org (open-source, zlib).

# License

Puppeteer is published under the MIT license, which allows you to do pretty
much everything (including making changes and selling binaries). However
bug fixes, improvements, or other changes are most welcome!

Here is the full license text:

    Puppeteer - A Motion Capture Mapping Tool
    Copyright (c) 2013-2015 Martin Felis <martin.felis@iwr.uni-heidelberg.de>.
    All rights reserved.
    
    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:
    
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
