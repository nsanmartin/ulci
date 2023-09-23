NTESTS=10000

RED='\033[0;31m'
NC='\033[0m' # No Color

ANY_ERROR=false

make build/parser

while read ln; do
    OUT=$(echo $ln |./build/parser 2>&1)
    RV=$?
    if [[ 0 -ne $RV ]]; then
        echo -e ${RED}Error: $RV${NC} on input: \"$ln\"
    fi
done < <(cat /dev/urandom | tr -cd 'xyz .\\\n' | head -c $NTESTS)


if [[ $ANY_ERROR == "false" ]]; then
    echo $NTESTS tests Ok
fi

