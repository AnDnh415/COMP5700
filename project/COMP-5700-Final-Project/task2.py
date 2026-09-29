import pandas as pd
import json
import csv

# Load datasets
all_repo_df = pd.read_parquet("hf://datasets/hao-li/AIDev/all_repository.parquet", engine="pyarrow")

print("Data loaded for Tasks 2")

# --- Task 2 ---
# REPOID: Data related to `id`
# LANG: Data related to `language`
# STARS: Data related to `stars`
# REPOURL: Data related to `url`
print("Processing Task 2...")
df_task2 = all_repo_df[['id', 'language', 'stars', 'url']]
df_task2 = df_task2.rename(columns={
    'id': 'REPOID',
    'language': 'LANG',
    'stars': 'STARS',
    'url': 'REPOURL'
})
df_task2.to_csv('task_2_output.csv', index=False)
print("Task 2 complete.")