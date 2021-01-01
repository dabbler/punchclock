#!/bin/bash
REV=$(svn info | grep "Last Changed Rev" | awk '{ print $4 }')
cat version.tmpl | sed "s/.WCREV./$REV/" > /tmp/version.h
diff /tmp/version.h version.h
if [ $? -ne 0 ]
then
	mv -f /tmp/version.h version.h
	echo Updated to Rev: $REV
fi
