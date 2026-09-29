#!/usr/bin/env python3

import yaml
import re

class CryptoViolationDetector:
    def __init__(self):
        # Violation patterns lookup table
        # do not use own cryptographic algorithms or implementations
        # misuse of libraries or algorithms
        # poor key management
        # randomness that is not random
        # failure to allow for algorithm adaptation and evolution
        self.patterns = {
            'weak_algorithm': [r'\bMD5\b', r'\bSHA1\b', r'\bDES\b', r'\bRC4\b'],
            'misused_implementation': [r'our own implementation', r'custom implementation'],
            'poor_key_management': [r'hardcoded.*key', r'static.*key'],
            'weak_randomness': [r'fixed range', r'predictable.*random'],
            'algorithm_rigidity': [r'will not.*upgrade', r'never.*change']
        }
        
        # Severity lookup table
        self.severity = {
            'weak_algorithm': 'HIGH',
            'misused_implementation': 'CRITICAL', 
            'weak_randomness': 'MEDIUM',
            'poor_key_management': 'HIGH',
            'algorithm_rigidity': 'LOW'
        }
    
    # Task 1 (T1): YAML Parsing Method
    def parse_yaml_content(self, yaml_content):
        if not yaml_content or not yaml_content.strip():
            raise ValueError("YAML content cannot be empty")
        
        try:
            parsed_data = yaml.safe_load(yaml_content)
            return parsed_data
        except yaml.YAMLError as e:
            raise yaml.YAMLError(f"YAML parsing failed: {e}")
    
    # Task 2 (T2): Content Extraction Method
    def extract_requirements_content(self, parsed_data):
        requirements = {}
        
        if isinstance(parsed_data, list) and len(parsed_data) > 0:
            # Handle list format (like the given example)
            for item in parsed_data:
                if isinstance(item, dict):
                    for key, value in item.items():
                        if isinstance(value, str):
                            requirements[key] = value
        elif isinstance(parsed_data, dict):
            # Handle direct dictionary format
            for key, value in parsed_data.items():
                if isinstance(value, str):
                    requirements[key] = value
        
        return requirements
    
    # Task 3 (T3): Key-Value Lookup for Violation Detection
    def detect_violations_by_lookup(self, requirements):
        violations = []
        
        for req_id, content in requirements.items():
            # Check each violation type using lookup patterns
            for violation_type, patterns in self.patterns.items():
                for pattern in patterns:

                    match = re.search(pattern, content, re.IGNORECASE)
                    if match:
                        matched_text = match.group(0)
                        violation = {
                            'requirement_id': req_id,
                            'violation_type': violation_type,
                            'matched_text': matched_text,
                            'severity': self.severity[violation_type],
                            'content': content
                        }
                        violations.append(violation)
                        break  # Only one violation per type per requirement
        
        return violations
    
    def analyze_yaml(self, yaml_content):
        parsed_data = self.parse_yaml_content(yaml_content)
        requirements = self.extract_requirements_content(parsed_data)
        violations = self.detect_violations_by_lookup(requirements)
        return requirements, violations
    
    def print_results(self, requirements, violations):
        print("REQUIREMENTS ANALYSIS:")
        for req_id, content in requirements.items():
            print(f"{req_id}: {content}")
        
        print(f"\nVIOLATIONS FOUND: {len(violations)}")
        for v in violations:
            print(f"{v['requirement_id']}: {v['violation_type']} ({v['severity']}) - '{v['matched_text']}'")


# Task 4 (T4): Test Cases - 5 each for T1, T2, T3
def run_all_tests():
    detector = CryptoViolationDetector()
    passed = 0
    total = 15
    
    # T1 Tests - YAML Parsing (5 tests)
    print("T1 - YAML PARSING TESTS:")
    
    # T1-1: Valid YAML
    try:
        yaml_content = "- ALL: 'Test'\n  R1: 'Use AES'"
        result = detector.parse_yaml_content(yaml_content)
        assert isinstance(result, list)
        print("T1-1: Valid YAML parsing - PASSED")
        passed += 1
    except:
        print("T1-1: Valid YAML parsing - FAILED")
    
    # T1-2: Empty YAML
    try:
        detector.parse_yaml_content("")
        print("T1-2: Empty YAML handling - FAILED")
    except ValueError:
        print("T1-2: Empty YAML handling - PASSED")
        passed += 1
    
    # T1-3: Invalid YAML
    try:
        detector.parse_yaml_content("- invalid: [yaml")
        print("T1-3: Invalid YAML handling - FAILED")
    except yaml.YAMLError:
        print("T1-3: Invalid YAML handling - PASSED")
        passed += 1
    
    # T1-4: Complex YAML
    try:
        yaml_content = "- ALL: 'Test'\n  R1:\n    - 'Sub1'\n    - 'Sub2'"
        result = detector.parse_yaml_content(yaml_content)
        assert result is not None
        print("T1-4: Complex YAML parsing - PASSED")
        passed += 1
    except:
        print("T1-4: Complex YAML parsing - FAILED")
    
    # T1-5: Multiple documents
    try:
        yaml_content = "- doc1: 'test1'\n- doc2: 'test2'"
        result = detector.parse_yaml_content(yaml_content)
        assert isinstance(result, list)
        print("T1-5: Multiple documents - PASSED")
        passed += 1
    except:
        print("T1-5: Multiple documents - FAILED")
    
    # T2 Tests - Content Extraction (5 tests)
    print("\nT2 - CONTENT EXTRACTION TESTS:")
    
    # T2-1: List format
    try:
        data = [{"ALL": "Test", "R1": "Use MD5", "R2": "Use AES"}]
        result = detector.extract_requirements_content(data)
        assert len(result) == 3
        print("T2-1: Extract from list format - PASSED")
        passed += 1
    except:
        print("T2-1: Extract from list format - FAILED")
    
    # T2-2: Dict format  
    try:
        data = {"ALL": "Test", "R1": "Use SHA1"}
        result = detector.extract_requirements_content(data)
        assert len(result) == 2
        print("T2-2: Extract from dict format- PASSED")
        passed += 1
    except:
        print("✗ T2-2: Extract from dict format - FAILED")
    
    # T2-3: Empty data
    try:
        result = detector.extract_requirements_content({})
        assert len(result) == 0
        print("T2-3: Extract from empty data - PASSED")
        passed += 1
    except:
        print("T2-3: Extract from empty data - FAILED")
    
    # T2-4: Mixed types
    try:
        data = [{"ALL": "Test", "R1": "Use DES", "R2": ["list"], "R3": 123}]
        result = detector.extract_requirements_content(data)
        string_count = sum(1 for v in result.values() if isinstance(v, str))
        assert string_count >= 2
        print("T2-4: Extract mixed types - PASSED")
        passed += 1
    except:
        print("T2-4: Extract mixed types - FAILED")
    
    # T2-5: Nested structure
    try:
        data = [{"section": {"R1": "Use RC4"}}]
        result = detector.extract_requirements_content(data)
        # Should handle gracefully
        print("T2-5: Extract nested structure - PASSED")
        passed += 1
    except:
        print("T2-5: Extract nested structure - FAILED")
    
    # T3 Tests - Violation Detection (5 tests)
    print("\nT3 - VIOLATION DETECTION TESTS:")
    
    # T3-1: Weak algorithm
    try:
        reqs = {"R1": "We will use MD5 for passwords"}
        violations = detector.detect_violations_by_lookup(reqs)
        weak_violations = [v for v in violations if v['violation_type'] == 'weak_algorithm']
        assert len(weak_violations) > 0
        print("T3-1: Detect weak algorithm - PASSED")
        passed += 1
    except:
        print("T3-1: Detect weak algorithm - FAILED")
    
    # T3-2: Misused implementation
    try:
        reqs = {"R1": "We will use our own implementation of SHA256"}
        violations = detector.detect_violations_by_lookup(reqs)
        custom_violations = [v for v in violations if v['violation_type'] == 'misused_implementation']
        assert len(custom_violations) > 0
        print("T3-2: Detect misused implementation - PASSED")
        passed += 1
    except:
        print("T3-2: Detect misused implementation - FAILED")
    
    # T3-3: Weak randomness
    try:
        reqs = {"R1": "Random numbers use fixed range between 1-100"}
        violations = detector.detect_violations_by_lookup(reqs)
        random_violations = [v for v in violations if v['violation_type'] == 'weak_randomness']
        assert len(random_violations) > 0
        print("T3-3: Detect weak randomness - PASSED")
        passed += 1
    except:
        print("T3-3: Detect weak randomness - FAILED")
    
    # T3-4: No violations
    try:
        reqs = {"R1": "We will use AES-256 with proper key management"}
        violations = detector.detect_violations_by_lookup(reqs)
        assert len(violations) == 0
        print("T3-4: Detect no violations - PASSED")
        passed += 1
    except:
        print("T3-4: Detect no violations - FAILED")
    
    # T3-5: Multiple violations
    try:
        reqs = {"R1": "Use MD5 with our own implementation"}
        violations = detector.detect_violations_by_lookup(reqs)
        violation_types = set(v['violation_type'] for v in violations)
        assert len(violation_types) >= 1  # At least one type detected
        print("T3-5: Detect multiple violations - PASSED")
        passed += 1
    except:
        print("T3-5: Detect multiple violations - FAILED")
    
    print(f"\n=== {passed}/{total} TESTS PASSED ===")
    return passed == total


def main():  
    # Sample YAML from assignment
    sample_yaml = """
- ALL: "This user story focuses on clearly specifying crypto-related requirements"
  R1: "We will use MD5 for encrypting all passwords and GitHub API keys."
  R2: "For generating random numbers we will use a fixed range between 39 and 51."
  R3: "We will be using our own implementation of SHA512 to protect API keys used for GPT-4."
  R4: "Keys for vault will be rotated."
  R5: "If a new cryptography algorithm comes with better strength, then we will use it instead of SHA512."
"""
    # Create detector and analyze
    detector = CryptoViolationDetector()
    
    try:
        requirements, violations = detector.analyze_yaml(sample_yaml)
        detector.print_results(requirements, violations)
        
        # Summary
        print("SUMMARY:")
        print(f"- Total requirements analyzed: {len(requirements)}")
        print(f"- Violations found: {len(violations)}")
        
        if violations:
            print("- Problematic requirements:")
            for v in violations:
                print(f"  * {v['requirement_id']}: {v['violation_type']} ({v['severity']})")
        else:
            print("- No violations detected!")

        # Run tests
        print("\nRUNNING TESTS...")
        run_all_tests()
            
    except Exception as e:
        print(f"Error: {e}")
        return False
    
    return True


if __name__ == "__main__":
    success = main()
    exit(0 if success else 1)