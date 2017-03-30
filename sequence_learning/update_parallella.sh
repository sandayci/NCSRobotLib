readonly PARALLELLA_IP=10.162.177.221
readonly FOLDER_NAME=sequence_new

rsync -v -rv -e ssh --exclude='.git/*' --exclude='build' \
    --exclude='NCSRobotLib/' --exclude='.*' \
    ./ \
    root\@$PARALLELLA_IP:/home/parallella/code/$FOLDER_NAME

ssh root@$PARALLELLA_IP mkdir /home/parallella/code/$FOLDER_NAME/build
ssh root@$PARALLELLA_IP cmake -B/home/parallella/code/$FOLDER_NAME/build -H/home/parallella/code/$FOLDER_NAME
ssh root@$PARALLELLA_IP make --directory /home/parallella/code/$FOLDER_NAME/build/ move_little_pushbot_lib/fast move_little_pushbot/fast
