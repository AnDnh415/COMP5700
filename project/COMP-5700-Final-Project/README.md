README.md

# Mining dataset AI Pull Requests
'Task1.py': script to extract and process Pull Request data
* TITLE: Data related to `title`
* ID: Data related to `id`
* AGENTNAME: Data related to `agent`
* BODYSTRING: Data related to `body`
* REPOID: Data related to `repo_id`
* REPOURL: Data related to `repo_url`

'Task2.py': script to extract and process Repository data
* REPOID: Data related to `id`
* LANG: Data related to `language`
* STARS: Data related to `stars`
* REPOURL: Data related to `url`

'Task3.py': script to extract and process Task Type data
* PRID: Data related to `id`
* PRTITLE: Data related to `title`
* PRREASON: Data related to `reason`
* PRTYPE: Data related to `type`
* CONFIDENCE: Data related to `confidence`

'Task4.py': script to extract and process Commit Details
* PRID: Data related to `pr_id`
* PRSHA: Data related to `sha`
* PRCOMMITMESSAGE: Data related to `message`
* PRFILE: Data related to `filename`
* PRSTATUS: Data related to `status`
* PRADDS: Data related to `additions`
* PRDELSS: Data related to `deletions`
* PRCHANGECOUNT: Data related to `changes`
* PRDIFF: Data related to `patch`. Please remove special characters in the diff to avoid string encoding errors. 

## Install Requirements
pip install pandas pyarrow huggingface_hub<br>import csv<br>import json

## Data source
[hao-li/AIDev] (https://huggingface.co/datasets/hao-li/AIDev)<br>Tables used for task 1 and 3: 'all_pull_requests', 'pr_task_type'<br>Tables used for task 2 and 4: 'all_repository', 'pr_commit_details'

## Tasks assignment
Task 1 & 3 are done by Sarabeth Shannon<br>
Task 2 & 4 are done by An Dinh<br>
Both attempted to do task 5 and agreed to use An's source code