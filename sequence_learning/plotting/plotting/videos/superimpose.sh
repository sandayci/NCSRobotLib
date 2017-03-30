echo "get frame from video every second"
mkdir helper
rm -rf ./helper/*
ffmpeg -i $1 -vf fps=$2 ./helper/out%3d.png

echo "combine them together to result.jpg"

images="convert -compose darken"
i="first"
for entry in `ls helper`; do
    if [ $i = "first" ]; then
        images="$images ./helper/$entry"
        i="second"
    else
        images="$images ./helper/$entry -composite"
    fi
done

eval "$images result.jpg"
