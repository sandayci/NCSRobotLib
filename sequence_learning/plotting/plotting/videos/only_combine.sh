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
