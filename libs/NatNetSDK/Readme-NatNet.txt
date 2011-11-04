NaturalPoint, Inc.

NatNet SDK is a simple Client/Server networking SDK for sending and receiving
NaturalPoint data across networks.  NatNet uses the UDP protocol in conjunction
with either multicasting or point-to-point unicasting for transmitting data.

Please refer to the NatNet API USer's Guide for more information.


Change Log
-----------

Version 2.2.0 (4/25/2010)
------------------------------
Added:   New Unicast Point-to-Point connection type.  Servers and clients can now 
         use MultiCast or Unicast as their connection type.  Connection type between
         server and client must be the same. 

Added:   Application-definable command and data port assignments, including Multicast address.
         
Added:   VC redistributable installer (\Samples\VCRedist\vcredist_x86.exe) for running
         the pre-compiled samples on machines without VisualStudio and/or the correct
         version of the CRT runtime installed on them.

Changed: Changed PacketClient to support shared addresses (SO_REUSEADDR).  Necessary
         when server and client are running on same machine.

Changed: Updates to SimpleServer, SampleClient, and Winforms client to illustrate
         Unicast usage.

Changed: Default Multicast address/port to IANA safe default values.

Fixed:   Precompiled WinForms sample crashes on some machines.



Version 2.1.0 (11/20/2009)
------------------------------
Added:   New "Skeleton" data type, representing a named, hierarchical collection
         of RigidBodies.


Version 2.0.0 (11/12/2009)
------------------------------       
Added:   New RigidBody parameters
          - RigidBody Name to RigidBody description
          - MarkerData to RigidBody Data, including ID, position, and size
          - MeanError to RigidBody data

Added:   New managed (.NET) class library (NatNetML.dll).  Allows for NatNetClient
         and NatNet data types to be consumed directly in .NET compatible
         environments (e.g. VB.Net, C#, LabView, MatLab).
       
Added:   New WinForms .NET sample application illustrating NatNetML consumption.

Added:   New depacketization sample (PacketClient) to replace the bitstream syntax.
         This sample can be used to decode NatNet packets directly without the need 
         for the NatNet SDK.  Intended only for clients that cannot use the NatNet SDK (e.g. Unix clients).

Added:   New SDK documentation.

Added:   Basic Client/Server message passing support (SendMessage(..)/SendMessageAndWait(..))

Added:   Allow connections to a single server from multiple clients on same and/or different machines.

Changed: SampleClient updated to illustrate MarkerSet and RigidBody data handling.

Changed: SampleServer and SimpleServer samples merged into a single, simplified NatNet Server example.

Changed: SampleClient3D to include conversion and display of RigidBody quaternions to euler angles.

Changed: Updated to IPV6 protocol.

        
Version 1.2.0 (1/23/2008)
------------------------------
Added:   VC8 static library (NatNetLibStatic.lib)

Added:   VC6 static library (NatNetvc6StaticLib.lib)

Added:   x64 libraries (dynamic and static)

Added:   Bitstream syntax documentation.

Added:   Rigid body data type support.

Added:   Versioning information.  NatNet version information is now available from
         file (DLL), local version (NatNetHelper::NatNetVersion), and fom the
         connected application(ServerDescription.NatNetVersion()) to help ensure NatNet
         server/client versions are in synch.

Changed: Updated to Visual Studio 2005 Project files.

Changed: Removed all /clr references.  NatNet and the samples use strictly
         native code.

Changed: NatNet types and helper routines cleaned up to improve usage.

Changed: SampleServer and Sampleclient programs updated to illustrate new usages.

