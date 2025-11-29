# Example Run

This document records an example run of the `enhancer-cli` using the "Stub" model mode (for verification without heavy weights).

## Command

```powershell
.\bin\enhancer-cli.exe --input "sample.jpg" --output "sample_upscaled.png" --model "models/stub.onnx" --scale 4
```

## Expected Output

```text
Initializing engine...
[Mock] Loading stub model...
Processing file...
Success!
```

## Notes

- The `stub.onnx` does not need to exist physically if the code detects the "stub" keyword in the path, but for consistency, you can create an empty file named `models/stub.onnx`.
- The output image `sample_upscaled.png` will be a grey/dummy image in this mock mode, but it verifies that the pipeline (Load -> Tile -> Inference -> Merge -> Save) is functioning correctly.
