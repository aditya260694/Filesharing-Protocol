awk 'BEGIN {print "\tMD5HASH VALUE & NAME \t\t\t\tTIMESTAMP"}'
if [ -f "$1" ]	
then
	echo `md5sum $1` "     " `stat -c%y $1`
else
	echo "Enter a Valid File Name!"
fi
