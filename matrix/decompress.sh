tarfiles=`ls *.tar.gz`
for tarfile in $tarfiles
do
    tar zxf $tarfile
done

directories=`ls -F | grep /`
for directory in $directories
do
    mv $directory/${directory/%?/}.mtx $PWD && rm -rf $directory
done

