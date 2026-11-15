import testframework as tf
import tempfile
from pathlib import Path


class RandomScaleRegressionTest(tf.UiTest):
    def test_global_shortcuts_keep_original_scale_and_notes(self):
        c = self.controller
        p = self.env.sequencer.model.project
        p.setTrackMode(0, p.tracks[0].TrackMode.Note)
        p.selectedTrackIndex = 0
        p.scale = 2  # Minor
        p.rootNote = 7
        sequence = p.selectedNoteSequence
        self.assertEqual(sequence.scale, -1)  # Inherits the Project scale.
        sequence.steps[0].note = 17

        c.selectPage("steps")
        previous = p.scale
        for _ in range(5):
            c.down("shift").press("perf").up("shift").wait(20)
            self.assertNotEqual(p.scale, previous)
            self.assertLess(p.scale, 19)
            self.assertEqual(p.rootNote, 7)
            self.assertEqual(sequence.steps[0].note, 17)
            self.assertTrue(self.env.sequencer.isNoteSequenceEditPageTop)
            previous = p.scale

        with tempfile.TemporaryDirectory() as directory:
            filename = str(Path(directory) / "random-scale-project.pro")
            p.save(filename)
            loaded = tf.sequencer.Project()
            loaded.load(filename)
            self.assertEqual(loaded.scale, 2)
            self.assertEqual(loaded.rootNote, 7)
            self.assertEqual(loaded.tracks[0].noteTrack.sequences[0].steps[0].note, 17)

        c.selectPage("project")
        c.down("shift").press("patt").up("shift").wait(20)
        self.assertEqual(p.scale, 2)
        self.assertEqual(p.rootNote, 7)
        self.assertEqual(sequence.steps[0].note, 17)
        self.assertFalse(self.env.sequencer.isModalPageTop)
