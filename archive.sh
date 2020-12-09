proj_name=cspace

[[ -z "$1" ]] && {
  echo "Please pass version as argument"
  exit 1
}

path=./build/$proj_name-$1
git archive --prefix cspace/ --output $path.tar --format tar HEAD &&
git archive --prefix cspace/ --output $path.tar.gz --format tar.gz HEAD && {
  echo "Archive generated to $path"
  exit 0
}
