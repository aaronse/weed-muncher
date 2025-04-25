# Converts tab-delimited text from clipboard to Markdown table format and 
# copies it back to clipboard.  This script is designed to be run in a terminal
# or command line interface.
#
# Reads tab-delimited clipboard text, converts it to a pandas DataFrame, and 
# then formats it as a Markdown table, which is copied back to the clipboard 
# for easy pasting into Markdown editors or documents.

# https://chatgpt.com/c/680b269b-7d54-800b-bed9-542714f5cb96

import pandas as pd
import pyperclip
from io import StringIO

def tab_to_markdown():
    # Step 1: Read clipboard content
    tab_text = pyperclip.paste()

    # Step 2: Convert tab-delimited content to DataFrame
    df = pd.read_csv(StringIO(tab_text), sep='\t', dtype=str)
    df.fillna('', inplace=True)  # Replace NaN with blank

    # Step 3: Clean column headers
    df.columns = [col if not col.startswith("Unnamed:") else '' for col in df.columns]

    # Step 4: Convert to Markdown
    markdown_table = df.to_markdown(index=False)

    # Step 5: Copy Markdown to clipboard
    pyperclip.copy(markdown_table)
    print("✅ Markdown table copied to clipboard!")

if __name__ == "__main__":
    tab_to_markdown()
