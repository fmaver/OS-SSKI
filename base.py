import os

os.system("rm -fr /home/tosanchez/Dev/sski/config")
os.system("cp -r /home/tosanchez/Dev/sski/test/base/config /home/tosanchez/Dev/sski")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/base/BASE_1.txt 10")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/base/BASE_2.txt 20")
os.system("./build/console.out /home/tosanchez/Dev/sski/test/base/BASE_2.txt 20")
