declare function print(s: string): void;
declare function vibrate(): void;
declare function delay(ms: number): void;
declare function getBrightness(): number;
declare function setBrightness(value: number): void;
declare function getAccelerometer(): { x: number, y: number, z: number } | null;
declare function http(method: string, url: string, payload?: string): { code: number, body: string };
