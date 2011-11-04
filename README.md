PyNatNet
========

A Python 2.6 wrapper around the [NatNet SDK](http://www.naturalpoint.com/optitrack/products/natnet-sdk/)

Installation
------------

This wrapper only works on Win32. 
Use Python distutils to build and install the wrapper. The NatNet SDK is included in this repository and does not need to be installed separately.

    $ git clone https://github.com/mkilling/PyNatNet.git
    $ cd PyNatNet
    $ python setup.py install

Usage
-----

```python
# create unicast client
client = NatNet.NatNetClient(1)
# connect to localhost
c.Initialize("", "")

# create and register a callback function
def onData(dataFrame):
    body = dataFrame.RigidBodies[0]
    print "x %.2f  y %.2f  z %.2f" % (body.x, body.y, body.z)

c.SetDataCallback(onData)

# wait while data frames come in
while True:
    pass
```

Caveats
-------

At the moment, the following caveats apply:

* The wrapper only works for rigid bodies. Most fields of sFrameOfMocapData are currently not exposed to Python.
* You cannot register message callbacks via NatNetClient::SetMessageCallback()
* You cannot query for the server description via NatNetClient::GetServerDescription()

License
-------

(Simplified BSD License)

Copyright 2011 Marvin Killing. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY MARVIN KILLING ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and should not be interpreted as representing official policies, either expressed or implied, of Marvin Killing.