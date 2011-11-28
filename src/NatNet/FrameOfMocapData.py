class FrameOfMocapData:
    def __init__(self, iFrame, rigidBodies, skeletons, latency):
        self.iFrame = iFrame
        self.RigidBodies = rigidBodies
        self.Skeletons = skeletons
        self.Latency = latency
