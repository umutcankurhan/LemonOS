unpack(){
    wget "https://www.python.org/ftp/python/3.8.2/Python-3.8.2.tgz"
 	tar -xzvf Python-3.8.2.tgz
 	export BUILD_DIR=Python-3.8.2
 	rm Python-3.8.2.tgz
}

buildp(){
 	cd $BUILD_DIR
 	patch -p1 < ../lemon-Python-3.8.2.patch
 	
	autoreconf
	CC=gcc CXX=g++ ./configure
	
	CC=gcc CXX=g++ make python Parser/pgen # You need python to build python so build host python
	mv python hostpython
	mv Parser/pgen Parser/hostpgen
	make distclean

    PYTHON_FOR_BUILD=./hostpython ac_cv_file__dev_ptmx=yes ac_cv_file__dev_ptc=no ./configure --host=x86_64-lemon --disable-ipv6 --build=x86_64 --prefix=/system --without-ensurepip --with-sysroot=$LEMON_SYSROOT
    
 	make -j$JOBCOUNT HOSTPYTHON=./hostpython HOSTPGEN=./Parser/hostpgen
 	make install DESTDIR=$LEMON_SYSROOT
}
