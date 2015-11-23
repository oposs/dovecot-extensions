#!/bin/sh

vagrant up
sleep 5
git diff remotes/origin/v2.2.19 remotes/origin/certificate-checking-2.2.19 > SOURCES/dovecot-2.2.19-fingerprint.patch
vagrant ssh -c "sudo /vagrant/build.sh"
vagrant sync-back
