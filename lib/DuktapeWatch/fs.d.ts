declare namespace fs {
	function readFile(path: string): string;
	function writeFile(path: string, s: string): number;
	function exists(path: string): boolean;
	function remove(path: string): boolean;
	function rename(from: string, to: string): boolean;
	function listDir(path: string): string[];
}
