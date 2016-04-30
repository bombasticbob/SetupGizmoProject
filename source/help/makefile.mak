#################################################################################
##                                                                             ##
##                     _           __  _  _                             _      ##
##   _ __ ___    __ _ | | __ ___  / _|(_)| |  ___     _ __ ___    __ _ | | __  ##
##  | '_ ` _ \  / _` || |/ // _ \| |_ | || | / _ \   | '_ ` _ \  / _` || |/ /  ##
##  | | | | | || (_| ||   <|  __/|  _|| || ||  __/ _ | | | | | || (_| ||   <   ##
##  |_| |_| |_| \__,_||_|\_\\___||_|  |_||_| \___|(_)|_| |_| |_| \__,_||_|\_\  ##
##                                                                             ##
##                                                                             ##
#################################################################################
##                                                                             ##
##         Copyright (c) 1998-2016 by S.F.T. Inc. - All rights reserved        ##
##    Use, copying, and distribution of this software are licensed according   ##
##      to the GPLv2, LGPLv2, or BSD license, as appropriate (see COPYING)     ##
##                                                                             ##
#################################################################################


# this is an nmake makefile.  It is part of the 'help' project for the
# S.F.T. Setup Gizmo (see above copyright and license).
# It creates the 'chm' help file from its source files.
# It uses the 'html help workshop' which should be pre-installed
# on any Windows 7 system.  As for Windows 'Ape' (8.x) or
# Win-10-nic (10) I haven't got any idea whether they have it.
# You'll see the path for hhc hardcoded at the end of this file.  If
# you need to, just modify that path.

all:  Output\SetupGiz.chm

clean:
	-del Output\SetupGiz.chm

rebuild:  all


DEPENDS = SetupGiz.hhp
DEPENDS = $(DEPENDS) SetupGiz.hhc
DEPENDS = $(DEPENDS) SetupGiz.html
DEPENDS = $(DEPENDS) application.html
DEPENDS = $(DEPENDS) BugoSoftNotYet.html
DEPENDS = $(DEPENDS) building.html
DEPENDS = $(DEPENDS) certificates.html
DEPENDS = $(DEPENDS) CertMgr.html
DEPENDS = $(DEPENDS) create_cert_authority.html
DEPENDS = $(DEPENDS) create_signing_cert.html
DEPENDS = $(DEPENDS) command_lines.html
DEPENDS = $(DEPENDS) giz_format.html
DEPENDS = $(DEPENDS) mdac.html
DEPENDS = $(DEPENDS) nt_services.html
DEPENDS = $(DEPENDS) ole_object.html
DEPENDS = $(DEPENDS) optional_components.html
DEPENDS = $(DEPENDS) overview.html
DEPENDS = $(DEPENDS) process.html
DEPENDS = $(DEPENDS) self_register.html
DEPENDS = $(DEPENDS) setup_inf.html
DEPENDS = $(DEPENDS) signed_cabinets.html
DEPENDS = $(DEPENDS) signing_with_cert.html
DEPENDS = $(DEPENDS) specifying_paths.html
DEPENDS = $(DEPENDS) speech_api.html
DEPENDS = $(DEPENDS) subsystem.html
DEPENDS = $(DEPENDS) wininet.html
DEPENDS = $(DEPENDS) res\autorun.jpg
DEPENDS = $(DEPENDS) res\certmgr.jpg
DEPENDS = $(DEPENDS) res\flowchart.jpg
DEPENDS = $(DEPENDS) res\option_install.jpg
DEPENDS = $(DEPENDS) res\screen1.jpg
DEPENDS = $(DEPENDS) res\screen1a.jpg
DEPENDS = $(DEPENDS) res\screen2.jpg
DEPENDS = $(DEPENDS) res\screen3.jpg
DEPENDS = $(DEPENDS) res\screen3a.jpg
DEPENDS = $(DEPENDS) res\screen3b.jpg
DEPENDS = $(DEPENDS) res\screen4.jpg
DEPENDS = $(DEPENDS) res\screen5.jpg

Output\SetupGiz.chm:	$(DEPENDS)
	-"mkdir Output"
	-"C:\Program Files (x86)\HTML Help Workshop\hhc" SetupGiz.hhp


