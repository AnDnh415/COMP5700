import pandas as pd
import numpy as np
import sys
from sklearn.model_selection import RepeatedKFold, cross_validate
from sklearn.tree import DecisionTreeClassifier
from sklearn.preprocessing import LabelEncoder
from sklearn.impute import SimpleImputer

DATA_FILE = 'DATASET.csv'
METRICS_FILE = 'classification_result.txt'
IMPORTANCE_FILE = 'feature_importance.txt'

try:
    # item 1: load data
    print(f"Loading data from {DATA_FILE}...")
    df = pd.read_csv(DATA_FILE)

    # separate features and target
    y = df['Type']
    x = df.drop(columns=['Type', 'URL'])

    # handle missing values
    num_col = x.select_dtypes(include = np.number).columns
    num_imputer = SimpleImputer(strategy='mean')
    x[num_col] = num_imputer.fit_transform(x[num_col])

    cat_col = x.select_dtypes(include = 'object').columns
    cat_imputer = SimpleImputer(strategy='most_frequent')
    x[cat_col] = cat_imputer.fit_transform(x[cat_col])

    # encode categorical variables
    for col in cat_col:
        x[col] = LabelEncoder().fit_transform(x[col])
    print(f"Data loaded and preprocessed: {x.shape[0]} samples, {x.shape[1]} features.")

    # item 2: define decision tree with entropy
    model = DecisionTreeClassifier(criterion='entropy', random_state=42)

    # item 3: 10x10-fold cross-validation
    cv = RepeatedKFold(n_splits=10, n_repeats=10, random_state=42)
    scoring = ['accuracy', 'precision', 'recall', 'f1']
    scores = cross_validate(model, x, y, scoring = scoring, n_jobs=-1)

    avg_metrics = {
        'Accuracy': np.mean(scores['test_accuracy']),
        'Precision': np.mean(scores['test_precision']),
        'Recall': np.mean(scores['test_recall']),
        'F1-Score': np.mean(scores['test_f1'])
    }

    with open(METRICS_FILE, 'w') as f:
        f.write("Classification Metrics (10x10-fold CV):\n")
        print("Classification Metrics (10x10-fold CV):")
        for metric, value in avg_metrics.items():
            line = f"{metric}: {value:.4f}\n"
            print(line.strip())
            f.write(line + "\n")
    print(f"Metric results saved to {METRICS_FILE}.")

    # item 4: feature importance
    model.fit(x, y)
    importance = pd.Series(model.feature_importances_, index=x.columns)
    importance_sorted = importance.sort_values(ascending=False)

    with open(IMPORTANCE_FILE, 'w') as f:
        f.write("Feature Importances:\n")
        print("Feature Importances:")
        print(importance_sorted.to_string())
        f.write(importance_sorted.to_string())
    print(f"Feature importance results saved to {IMPORTANCE_FILE}.")

    print("Classification program completed.")

except FileNotFoundError:
    print(f"Error: The file {DATA_FILE} was not found.")
    sys.exit(1)
except KeyError as e:
    print(f"Error: Missing expected column in data - {e}")
    sys.exit(1)
except Exception as e:
    print(f"An error occurred: {e}")
    sys.exit(1)