set -x
ls -l .*.sh *.sh *.py .*.log *.log *.jpg
md5sum .*.sh *.sh *.py .*.log *.log *.jpg
./minifs/ul.sh .payload.log.1
./minifs/ul.sh .startup.log
touch .dopayload
