# SetupGizmoProject
The S.F.T. Setup Gizmo, a wizard-based SETUP creation utility for MS Windows

For licensing information, see LICENSE.

#Description
This program lets you quickly create a 'Setup' for your Windows application,
one that allows you to have custom options, OS-dependent installs, and a
self-extracting (downloadable) install.

The SETUP and UNINSTALL utilities are statically linked to prevent DLL Hell
problems, and are specially designed to be VERY VERY SMALL when compiled,
even with the statically-linked runtime.

The S.F.T. Setup Gizmo was once intended to be a commercial product.
Unfortunately it did not sell, probably because it was not marketed very well.
However, as I believe it has merit for OPEN SOURCE projects, I have released
it as an open source project itself.

The source is available in the 'source' directory.  Some required files are
missing; these are Microsoft owned components available in the Windows 7 SDK.

Binary images (self-extracting .exe) and the help file (.chm) are available
in the 'binaries' directory.


#Platform
The source was built on Windows 7 using DevStudio 2010 and the self-
extracting setup built using this version of the S.F.T. Setup Gizmo.  Any
other windows versions or compiler environments may not work correctly.

I choose to continue using Windows 7 because it is BETTER (in my opinion)
than either 8, 8.1, or 10.  Microsoft has changed too many things and I
do NOT like the changes.  I said the same thing when '.Net' was released.

So I will support these 'other things' but not with a lot of enthusiasm.


# Legacy
Much of the documentation is VERY old.  This program was originally written
around 1998, and as such has a LOT of legacy.  Most of the legacy has been
removed (XP or later supported) to avoid compiler-related problems caused
by attempting to support earlier versions of windows.  Some remains in
commented-out code, so if you want to modify it for earlier windows versions,
you have the source and are welcome to do so.


# ClassLib
The 'source/ClassLib' directory contains a library project that is in essence
an 'MFC Clone', written from scratch to comply with the functionality (and in
some cases, the naming) of MFC classes and functions.  The reason I did so
was that statically linked MFC has grown in size to about 1.5Mb which I think
is completely UNACCEPTABLE.  I want my executables to be *TINY*, not BLOATED.

If you want to make use of this 'ClassLib' project, it's also open source and
licensed in the same way as the rest of the code.  I happen to think that my
CString and CArray clones are pretty good, though not written for efficiency.
Again, it's supposed to be *SMALL*.


