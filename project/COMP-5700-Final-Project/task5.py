import pandas as pd

# Define the Security Keywords
SECURITY_KEYWORDS = [
    "race", "racy", "buffer", "overflow", "stack", 
    "integer", "signedness", "underflow", "improper", "unauthenticated", "gain access", 
    "permission", "cross site", "css", "xss", "denial service", "dos", "crash", "deadlock", 
    "injection", "request forgery", "csrf", "xsrf", "forged", "security", "vulnerability", 
    "vulnerable", "exploit", "attack", "bypass", "backdoor", "threat", "expose", "breach", 
    "violate", "fatal", "blacklist", "overrun", "insecure"]

def check_security(text):
    """
    Returns 1 if any keyword is found in the text, else 0.
    Case insensitive.
    """
    if not isinstance(text, str):
        return 0
    
    text_lower = text.lower()
    for keyword in SECURITY_KEYWORDS:
        if keyword.lower() in text_lower:
            return 1
    return 0

# Load the Data
print("Loading data...")

# Only need Task 1 (for Title/Body/Agent) and Task 3 (for Type/Confidence)
# Task 2 and 4 don't have specific columns requested in Task 5
df_task1 = pd.read_csv("task_1_output.csv")
df_task3 = pd.read_csv("task_3_output.csv")

# Merge the Dataframes
print("Merging data...")

# Task 1 uses 'ID' and Task 3 uses 'PRID' for the same identifier.
# Merge on these columns.
df_merged = pd.merge(
    df_task1, 
    df_task3, 
    left_on='ID', 
    right_on='PRID', 
    how='inner' # Keep only records that exist in both
)

# Create the 'SECURITY' Column
print("Analyzing keywords...")

# Check both TITLE and BODYSTRING
# Use apply() to run our check_security function row by row
df_merged['SECURITY'] = df_merged.apply(
    lambda row: 1 if (check_security(row['TITLE']) == 1 or check_security(row['BODYSTRING']) == 1) else 0,
    axis=1
)

# Format the Final Output
# Rename columns to match the requirement exactly
# Requirement: ID, AGENT, TYPE, CONFIDENCE, SECURITY
df_final = df_merged.rename(columns={
    'ID': 'ID',
    'AGENTNAME': 'AGENT',
    'PRTYPE': 'TYPE',
    'CONFIDENCE': 'CONFIDENCE'
    # SECURITY is already named SECURITY
})

# Select only the required columns
df_final_output = df_final[['ID', 'AGENT', 'TYPE', 'CONFIDENCE', 'SECURITY']]

output_filename = "task_5_output.csv"
df_final_output.to_csv(output_filename, index=False)

print(f"Task 5 complete.")