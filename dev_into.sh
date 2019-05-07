# !/bin/bash
docker build . -t convexwf/scut-2017-fall-os-labs

docker run -it --rm -v /$(pwd):/app convexwf/scut-2017-fall-os-labs bash