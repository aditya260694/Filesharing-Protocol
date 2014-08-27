awk 'BEGIN {print "\tMD5HASH VALUE & NAME \t\t\t\tTIMESTAMP"}'
for i in `ls`
do
	if [ -f "${i}" ]	
	then
		echo `md5sum $1` "     " `stat -c%y $1`
	else
		echo "Enter a Valid File Name!"
	fi
done
