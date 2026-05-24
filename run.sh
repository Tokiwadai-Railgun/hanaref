#!/usr/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "$(readlink -f "${BASH_SOURCE[0]}")" )" &> /dev/null && pwd )
PROJECT_DIR=$(pwd)
excluded=(--exclude=.git/)
if [ -f "$PROJECT_DIR/.gitignore" ] ; then
  while read -r line ; do
    [[ -z "$line" || "$line" == \#* ]] && continue
    if [ -d "$line" ] ; then
      excluded+=(--exclude-dir="$line")
    else
      excluded+=(--exclude="$line")
    fi
  done < "$PROJECT_DIR/.gitignore"
fi

result=$(grep -rhoE '\[[A-Za-z1-9 ]+\]\(inkdrop://note/[A-Za-z0-9-]+\)' "${excluded[@]}" "$PROJECT_DIR")
$SCRIPT_DIR/bin/tasks "$result" "$@"
