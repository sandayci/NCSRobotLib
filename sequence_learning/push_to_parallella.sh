readonly PARALLELLA_IP=10.162.177.221
readonly FOLDER_NAME=sequence_new

# check first whether the build would succeed
make --directory ./build/

ssh root@$PARALLELLA_IP rm -r /home/parallella/code//$FOLDER_NAME

rsync -v -rv -e ssh --exclude='.git/*' --exclude='build' \
    --exclude='NCSRobotLib/docs/' --exclude='.*' \
    ./ \
    root\@$PARALLELLA_IP:/home/parallella/code//$FOLDER_NAME

ssh root@$PARALLELLA_IP mkdir /home/parallella/code//$FOLDER_NAME/build
ssh root@$PARALLELLA_IP cmake -B/home/parallella/code//$FOLDER_NAME/build -H/home/parallella/code//$FOLDER_NAME
ssh root@$PARALLELLA_IP make --directory /home/parallella/code/$FOLDER_NAME/build/
