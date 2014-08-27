x=$1
y=$2
awk 'BEGIN {print "FILENAME \t\t\t\t\t FILESIZE \t\t\t TIMESTAMP"}' > outfile
ls -l --time-style=+%Y%m%d%H%M%S -t | awk -v x=$x -v y=$y '{if(x<=$6 && y>=$6) {printf "%-50s %-30s %s\n",$7,$5,$6}}' >> outfile
