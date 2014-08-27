awk 'BEGIN {print "FILENAME \t\t\t\t\t FILESIZE \t\t\t TIMESTAMP"}' > outfile
ls -l | awk '{ s = ""; for (i = 9; i <= NF; i++) s = s $i " "; printf "%-50s %-30s %s %s %s\n", s, $5,$6,$7,$8}' >> outfile
