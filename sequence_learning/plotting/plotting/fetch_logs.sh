readonly PARALLELLA_IP=10.162.177.221
readonly PARALLELLA_LOGS=/home/parallella/code/sequence_new/logs/*
readonly HOST_LOG_DIR=./logs

rsync -v -rv -e ssh root\@$PARALLELLA_IP:$PARALLELLA_LOGS $HOST_LOG_DIR  --remove-source-files
