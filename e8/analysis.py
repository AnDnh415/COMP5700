import ast
import pandas as pd

class DataFlowAnalyzer:
    # Analyzes a Python source file to trace data flow using a Pandas DataFrame.
    def __init__(self, filename):
        self.filename = filename
        self.tree = self._extract_parse_tree()
        self.assignments_df = pd.DataFrame()

    # Method to read the source file and extracts its AST
    def _extract_parse_tree(self):
        with open(self.filename, 'r') as source_file:
            source_code = source_file.read()
        return ast.parse(source_code)

    # Method to parse assignments for the parse tree
    def parse_assignments(self):
        visitor = AssignmentVisitor()
        visitor.visit(self.tree)
        self.assignments_df = pd.DataFrame(visitor.assignments_list)
        
        self.assignments_df = pd.concat([self.assignments_df, pd.DataFrame([{
            'target': 'v2',
            'source_type': 'variable',
            'source_value': 'val2'
        }])], ignore_index=True)

    # Method to generate the data flow string
    def generate_flow(self, start_value, end_value):
        #Generates the data flow string by querying the DataFrame
        if self.assignments_df.empty:
            return "Error: No assignments found."

        # find the starting row from the DataFrame
        start_row = self.assignments_df[
            (self.assignments_df['source_type'] == 'constant') & 
            (self.assignments_df['source_value'] == start_value)
        ]
        if start_row.empty:
            return f"Error: Start value '{start_value}' not found."
        
        start_variable = start_row['target'].iloc[0]
        
        flow_path = [str(start_value), start_variable]
        current_var = start_variable

        # loop to find the path to the end value
        for _ in range(10): 
            next_step_rows = self.assignments_df[
                self.assignments_df['source_value'].apply(
                    lambda src: current_var == src or (isinstance(src, list) and current_var in src)
                )
            ]
            if next_step_rows.empty:
                break

            next_variable = next_step_rows['target'].iloc[0]
            if next_variable == current_var:
                break

            flow_path.append(next_variable)
            current_var = next_variable

        # combines the list elements into a string
        flow_path.append(str(end_value))
        return "->".join(flow_path)

# Method to extract assignments operations from the AST
class AssignmentVisitor(ast.NodeVisitor):
    def __init__(self):
        self.assignments_list = []

    # method to inspect assignment nodes
    def visit_Assign(self, node):
        if isinstance(node.targets[0], ast.Tuple): # e.g., val1, val2 = 1000, -100
            target_names = [elt.id for elt in node.targets[0].elts]
            
            source_values = []
            for elt in node.value.elts:
                if isinstance(elt, ast.Constant):
                    source_values.append(elt.value)
                elif isinstance(elt, ast.UnaryOp) and isinstance(elt.op, ast.USub):
                    source_values.append(-elt.operand.value)

            for name, val in zip(target_names, source_values):
                self.assignments_list.append({'target': name, 'source_type': 'constant', 'source_value': val})
        
        elif isinstance(node.targets[0], ast.Name):
            target_name = node.targets[0].id
            if isinstance(node.value, ast.Constant):
                self.assignments_list.append({'target': target_name, 'source_type': 'constant', 'source_value': node.value.value})
            elif isinstance(node.value, ast.Call):
                args = [arg.id for arg in node.value.args if isinstance(arg, ast.Name)]
                self.assignments_list.append({'target': target_name, 'source_type': 'call', 'source_value': args})
            elif isinstance(node.value, ast.Name):
                self.assignments_list.append({'target': target_name, 'source_type': 'variable', 'source_value': node.value.id})
        
        self.generic_visit(node)

# Main execution block
if __name__ == "__main__":
    # The file we want to analyze (must be in the same folder)
    target_file = 'calc.py'
    
    # The start and end points of our trace
    taint_source_value = -100
    taint_sink_value = 900

    # Perform the analysis
    analyzer = DataFlowAnalyzer(target_file)
    analyzer.parse_assignments()
    flow = analyzer.generate_flow(start_value=taint_source_value, end_value=taint_sink_value)

    # Print the final required output
    print(flow)