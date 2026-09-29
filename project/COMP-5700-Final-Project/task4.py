import pandas as pd
import json
import csv
import re
# --- Task 4 ---
print("Processing Task 4...")
commit_df = pd.read_parquet("hf://datasets/hao-li/AIDev/pr_commit_details.parquet", engine="pyarrow")
df_task4 = commit_df[['pr_id', 'sha', 'message', 'filename', 'status', 'additions', 'deletions', 'changes', 'patch']]
df_task4 = df_task4.rename(columns={
    'pr_id': 'PRID',
    'sha': 'PRSHA',
    'message': 'PRCOMMITMESSAGE',
    'filename': 'PRFILE',
    'status': 'PRSTATUS',
    'additions': 'PRADDS',
    'deletions': 'PRDELSS',
    'changes': 'PRCHANGECOUNT',
    'patch': 'PRDIFF'
})

# remove special characters to prevent csv issues
df_task4['PRDIFF'] = df_task4['PRDIFF'].astype(str).apply(lambda x: re.sub(r'[^\w\s\.\+\-\=\@\:]', ' ', x))
df_task4.to_csv('task_4_output.csv', index=False)
print("Task 4 complete.")