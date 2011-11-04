import NatNet

c = NatNet.NatNetClient(1)
print c.NatNetVersion()

c.SetVerbosityLevel(NatNet.Verbosity_Error)
