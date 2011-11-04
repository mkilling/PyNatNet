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
