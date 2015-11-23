HOWTO Build RPMS
==========================
Author: Manuel Oetiker 
Date:   20.02.2015, Version 2

Install RPM building tools
--------------------------

RHEL 6:

  $ yum install rpmdevtools

RHEL 5:

  $ yum install rpm-build
  
copy config file to .rpmmacros

  $ cp _rpmmacros ~/.rpmmacros
  $ vi ~/.rpmmacros

Building Dovecot
----------------

PreReqs

RHEL6:

 $ yum install pam-devel bzip2-devel libcap-devel libtool mysql-devel quota-devel xz-devel gettext-devel libcurl-devel postgresql-devel

RHEL 5:

 $ yum install pam-devel bzip2-devel libcap-devel postgresql84-devel mysql-devel quota-devel xz-devel gettext-devel curl-devel 

Build Packages:

 $ cd RPMBUILD

 $ rsync -av RPM/RHEL{5,6}/* RPMBUILD
 $ cd RPMBUILD

 $ rpmbuild -ba SPEC/dovecot.spec




