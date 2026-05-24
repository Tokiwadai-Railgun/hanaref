excluded="--exclude=.git/ "

if [ -f "./.gitignore" ] ; then
  while read -r line ; do
    if [ -d $line ] ; then
      excluded+=" --exclude-dir=$line"
    else
      excluded+=" --exclude=$line"
    fi
  done < .gitignore
fi

# -r recurse
# -h remove the filenames
# -o only return the matched part
# -E use regex
result=$(grep -rhoE '\[[A-Za-z1-9 ]+\]\(inkdrop://note/[A-Za-z0-9-]+\)' $excluded)
./bin/tasks "$result" $@
