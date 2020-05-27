sudo apt-get update
sudo apt-get -y install flex

cd /tmp
wget https://ftp.gnu.org/gnu/bison/bison-3.5.1.tar.gz
tar -zxvf bison-3.5.1.tar.gz
cd bison-3.5.1
./configure
sudo make
sudo make install
sudo ln -s /usr/local/bin/bison /usr/bin/bison

# sudo apt-get -y install bison=2:3.5.1+dfsg-1