#!/bin/bash
echo "Counting files in $(pwd):"
echo "--------------------------"
echo "Text files (.txt): $(ls *.txt 2>/dev/null | wc -l)"
echo "Shell scripts (.sh): $(ls *.sh 2>/dev/null | wc -l)"
echo "Markdown files (.md): $(ls *.md 2>/dev/null | wc -l)"

