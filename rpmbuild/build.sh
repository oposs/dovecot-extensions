#!/bin/sh
# Build Script 
# Manuel Oetiker <manuel@oetiker.ch>

BASE_PATH=/vagrant

cat > /etc/sysconfig/i18n << EOF
LANG="en_US.UTF-8"
SYSFONT="latarcyrheb-sun16"
EOF

cat > ~/.rpmmacros << EOF
%packager       Manuel Oetiker <manuel@oetiker.ch>
%vendor         OETIKER+PARTNER AG

%dist           .rh6

%__global_cflags  -O
%__global_ldflags -O

%_topdir       	 /vagrant

EOF

yum --assumeyes update
yum --assumeyes install pam-devel bzip2-devel libcap-devel libtool mysql-devel quota-devel xz-devel \
	    gettext-devel libcurl-devel postgresql-devel expat-devel openldap-devel \
	    rpm-build redhat-rpm-config make gcc 

if [ ! -f ${BASE_PATH}/SOURCES/dovecot-2.2.19.tar.gz ]
	then
        ( cd ${BASE_PATH}/SOURCES ; \
	  wget http://www.dovecot.org/releases/2.2/dovecot-2.2.19.tar.gz  )
fi

rpmbuild -v -ba ${BASE_PATH}/SPECS/dovecot.spec

