awk -v var="$*" '{if ($9==var) {print}}' sampledAccess > targetSet
