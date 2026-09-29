import pandas as pd
import yaml
import sys

# Read data from CSV file (already converted from Excel for ease of use)
def read_access_data(csv_file_path: str) -> pd.DataFrame:
    try:
        df = pd.read_csv(csv_file_path, index_col=0) 
        
        # Fill empty cells with 0 to represent "NO permission"
        df.fillna(0, inplace=True)
        
        # Ensure all permission data is treated as integers
        df = df.astype(int)
        
        return df
    except FileNotFoundError:
        print(f"Error: File not found at {csv_file_path}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error reading CSV file: {e}", file=sys.stderr)
        sys.exit(1)

# counts users with 777 permission for ALL 7 files.
def count_users_777_all(df: pd.DataFrame) -> int:
    return df.eq(777).all(axis=1).sum()

# counts users with 777 permission for AT LEAST ONE file
def count_users_777_at_least_one(df: pd.DataFrame) -> int:
    return df.eq(777).any(axis=1).sum()

# counts users with 444 permission for ALL 7 files.
def count_users_444_all(df: pd.DataFrame) -> int:
    return df.eq(444).all(axis=1).sum()

# counts users with 444 permission for AT LEAST ONE file
def count_users_444_at_least_one(df: pd.DataFrame) -> int:
    return df.eq(444).any(axis=1).sum()

# counts users with read permission for AT LEAST ONE file
def count_users_read_at_least_one(df: pd.DataFrame) -> int:
    # Any permission code where the hundreds digit is 4, 5, 6, or 7
    #(e.g., 400, 444, 555, 666, 777) grants read access.
    def grants_read(perm_code):
        # Check the owner's permission (the first digit)
        return (perm_code // 100) >= 4

    # .applymap() runs the function on every single cell
    # .any(axis=1) checks if any file in a row grants read access
    # .sum() counts how many users (rows) this is True for
    return df.applymap(grants_read).any(axis=1).sum()

# counts users with NO permission for ANY file
def count_users_no_permission_all(df: pd.DataFrame) -> int:
    return df.eq(0).all(axis=1).sum()

# construct hashmap for users with at least 2 permissions
def create_user_permission_hashmap(df: pd.DataFrame) -> dict:
    users_with_min_2_files = df[df.gt(0).sum(axis=1) >= 2]
    
    user_hashmap = {}

    for user_id, permissions in users_with_min_2_files.iterrows():
        # Get only the permissions that the user has (value > 0)
        user_permissions = permissions[permissions > 0].to_dict()
        
        # Add to main hashmap
        user_hashmap[user_id] = user_permissions
        
    return user_hashmap

# export hashmap to YAML file
def export_to_yaml(data_map: dict, output_file: str):
    try:
        with open(output_file, 'w') as f:
            # default_flow_style=False makes it human-readable
            yaml.dump(data_map, f, default_flow_style=False, sort_keys=False)
        print(f"Successfully exported data to {output_file}")
    except Exception as e:
        print(f"Error writing YAML file: {e}", file=sys.stderr)

if __name__ == "__main__":
    DATA_FILE = 'data.csv'
    YAML_FILE = 'user_permissions.yaml'

    print(f"Reading data from {DATA_FILE}...")
    main_df = read_access_data(DATA_FILE)

    print("--- Access Control Queries ---")
    print(f"Users with 777 for all files:\t{count_users_777_all(main_df)}")
    print(f"Users with 777 for at least one file:\t{count_users_777_at_least_one(main_df)}")
    print(f"Users with 444 for all files:\t{count_users_444_all(main_df)}")
    print(f"Users with 444 for at least one file:\t{count_users_444_at_least_one(main_df)}")
    print(f"Users with READ for at least one file:\t{count_users_read_at_least_one(main_df)}")
    print(f"Users with NO permission for any file:\t{count_users_no_permission_all(main_df)}")
    print("------------------------------\n")

    print("Generating user permission hashmap (for users with >= 2 permissions)...")
    user_map = create_user_permission_hashmap(main_df)
    print(f"Hashmap created for {len(user_map)} users.\n")

    print(f"Exporting hashmap to {YAML_FILE}...")
    export_to_yaml(user_map, YAML_FILE)