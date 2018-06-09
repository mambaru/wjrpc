echo valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $@
valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $@
EXIT_CODE=$?

echo "EXIT_CODE=$EXIT_CODE";

if [[ $EXIT_CODE -eq 1 ]]; then 
  echo "Memory leaks detected!";
  exit 1
fi

exit 0
