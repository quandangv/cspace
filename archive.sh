proj_name=cspace

# retrieve the project version from CMakeLists.txt
version=$(grep -oP 'project\(.*? VERSION \K[0-9.]+' CMakeLists.txt)
path=./build/$proj_name-$version
git archive --prefix $proj_name/ --output $path.tar --format tar HEAD &&
git archive --prefix $proj_name/ --output $path.tar.gz --format tar.gz HEAD &&{
  echo "Archive generated to $path"
  exit 0
}
